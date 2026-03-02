#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "myproject.h"

static std::vector<float> load_npy_f32(const std::string &path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Cannot open NPY file: " + path);
    }

    char magic[6];
    in.read(magic, 6);
    if (std::string(magic, 6) != "\x93NUMPY") {
        throw std::runtime_error("Invalid NPY magic");
    }

    uint8_t major = 0, minor = 0;
    in.read(reinterpret_cast<char *>(&major), 1);
    in.read(reinterpret_cast<char *>(&minor), 1);

    uint32_t header_len = 0;
    if (major == 1) {
        uint16_t hl16 = 0;
        in.read(reinterpret_cast<char *>(&hl16), 2);
        header_len = hl16;
    } else {
        in.read(reinterpret_cast<char *>(&header_len), 4);
    }

    std::string header(header_len, '\0');
    in.read(&header[0], header_len);

    if (header.find("'descr': '<f4'") == std::string::npos &&
        header.find("\"descr\": \"<f4\"") == std::string::npos) {
        throw std::runtime_error("Only little-endian float32 NPY supported");
    }

    auto l = header.find('(');
    auto r = header.find(')');
    if (l == std::string::npos || r == std::string::npos || r <= l) {
        throw std::runtime_error("Cannot parse shape from NPY header");
    }

    std::string shape = header.substr(l + 1, r - l - 1);
    size_t total = 1;
    size_t start = 0;
    while (start < shape.size()) {
        size_t comma = shape.find(',', start);
        std::string tok = shape.substr(start, comma == std::string::npos ? std::string::npos : comma - start);
        if (!tok.empty()) {
            size_t v = static_cast<size_t>(std::stoul(tok));
            if (v > 0) total *= v;
        }
        if (comma == std::string::npos) break;
        start = comma + 1;
        while (start < shape.size() && shape[start] == ' ') start++;
    }

    std::vector<float> out(total);
    in.read(reinterpret_cast<char *>(out.data()), total * sizeof(float));
    if (!in) {
        throw std::runtime_error("Could not read NPY payload");
    }
    return out;
}

int main() {
    axis_stream_t in_stream;
    axis_stream_t out_stream;

    std::vector<float> sample = load_npy_f32("sample.npy");
    if (sample.size() != INPUT_LEN) {
        std::cerr << "Unexpected sample size: " << sample.size() << std::endl;
        return 1;
    }

    for (int i = 0; i < INPUT_LEN; i++) {
        axis_pkt_t pkt;
        data_t v = (data_t)sample[i];
        pkt.data = v.range(15, 0);
        pkt.keep = -1;
        pkt.strb = -1;
        pkt.user = 0;
        pkt.id = 0;
        pkt.dest = 0;
        pkt.last = (i == INPUT_LEN - 1) ? 1 : 0;
        in_stream.write(pkt);
    }

    myproject(in_stream, out_stream, 1);

    axis_pkt_t out_pkt = out_stream.read();
    data_t out_val;
    out_val.range(15, 0) = out_pkt.data;

    std::cout << "Inference probability: " << (float)out_val << std::endl;

    // With placeholder zero weights, output should be sigmoid(0)=0.5
    const float expected = 0.5f;
    const float err = (float)out_val - expected;
    if (err > 0.05f || err < -0.05f) {
        std::cerr << "FAIL: expected ~" << expected << " got " << (float)out_val << std::endl;
        return 1;
    }

    std::cout << "PASS" << std::endl;
    return 0;
}
