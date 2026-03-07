`timescale 1ns/1ps

module cnn_tb;
    reg clk;
    reg rst;
    reg start;
    reg [187*16-1:0] ecg_in;
    wire done;
    wire [15:0] prob_out;

    reg [15:0] sample_mem [0:186];
    integer i;

    cnn_top dut (
        .clk(clk),
        .rst(rst),
        .start(start),
        .ecg_in(ecg_in),
        .done(done),
        .prob_out(prob_out)
    );

    always #5 clk = ~clk;

    initial begin
        clk = 0;
        rst = 1;
        start = 0;
        ecg_in = 0;

        $readmemh("verilog/weights/sample_input.mem", sample_mem);
        for (i = 0; i < 187; i = i + 1) begin
            ecg_in[i*16 +: 16] = sample_mem[i];
        end

        #30;
        rst = 0;
        #20;
        start = 1;
        #10;
        start = 0;

        #500;
        $finish;
    end

    initial begin
        $monitor("t=%0t rst=%0b start=%0b done=%0b prob_out(Q8.8)=0x%h", $time, rst, start, done, prob_out);
    end
endmodule
