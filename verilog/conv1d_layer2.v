module conv1d_layer2 #(
    parameter integer IN_LEN = 92,
    parameter integer IN_CH = 16,
    parameter integer OUT_CH = 32,
    parameter integer K = 3,
    parameter integer DATA_W = 16,
    parameter integer FRAC = 8,
    parameter integer OUT_LEN = IN_LEN-K+1
) (
    input  wire clk,
    input  wire rst,
    input  wire start,
    input  wire [IN_LEN*IN_CH*DATA_W-1:0] in_data,
    output reg done,
    output reg [OUT_LEN*OUT_CH*DATA_W-1:0] out_data
);
reg signed [DATA_W-1:0] w [0:K*IN_CH*OUT_CH-1];
reg signed [DATA_W-1:0] b [0:OUT_CH-1];
integer o, i, k, c;
reg signed [47:0] acc;

initial begin
    $readmemh("verilog/weights/conv2_w.mem", w);
    $readmemh("verilog/weights/conv2_b.mem", b);
end

always @(posedge clk) begin
    if (rst) begin
        done <= 1'b0;
    end else if (start) begin
        for (i = 0; i < OUT_LEN; i = i + 1) begin
            for (o = 0; o < OUT_CH; o = o + 1) begin
                acc = $signed(b[o]) <<< FRAC;
                for (k = 0; k < K; k = k + 1) begin
                    for (c = 0; c < IN_CH; c = c + 1) begin
                        acc = acc + $signed(in_data[((i+k)*IN_CH + c)*DATA_W +: DATA_W]) *
                                  $signed(w[(k*IN_CH + c)*OUT_CH + o]);
                    end
                end
                out_data[(i*OUT_CH + o)*DATA_W +: DATA_W] <= acc >>> FRAC;
            end
        end
        done <= 1'b1;
    end else begin
        done <= 1'b0;
    end
end
endmodule
