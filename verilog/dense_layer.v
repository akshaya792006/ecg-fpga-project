module dense_layer #(
    parameter integer DIN = 1440,
    parameter integer DOUT = 64,
    parameter integer DATA_W = 16,
    parameter integer FRAC = 8,
    parameter integer LAYER_ID = 1
) (
    input  wire clk,
    input  wire rst,
    input  wire start,
    input  wire [DIN*DATA_W-1:0] in_data,
    output reg done,
    output reg [DOUT*DATA_W-1:0] out_data
);
reg signed [DATA_W-1:0] w [0:DIN*DOUT-1];
reg signed [DATA_W-1:0] b [0:DOUT-1];
integer i, o;
reg signed [55:0] acc;

initial begin
    if (LAYER_ID == 1) begin
        $readmemh("verilog/weights/dense1_w.mem", w);
        $readmemh("verilog/weights/dense1_b.mem", b);
    end else begin
        $readmemh("verilog/weights/dense2_w.mem", w);
        $readmemh("verilog/weights/dense2_b.mem", b);
    end
end

always @(posedge clk) begin
    if (rst) begin
        done <= 1'b0;
    end else if (start) begin
        for (o = 0; o < DOUT; o = o + 1) begin
            acc = $signed(b[o]) <<< FRAC;
            for (i = 0; i < DIN; i = i + 1) begin
                acc = acc + $signed(in_data[i*DATA_W +: DATA_W]) *
                          $signed(w[i*DOUT + o]);
            end
            out_data[o*DATA_W +: DATA_W] <= acc >>> FRAC;
        end
        done <= 1'b1;
    end else begin
        done <= 1'b0;
    end
end
endmodule
