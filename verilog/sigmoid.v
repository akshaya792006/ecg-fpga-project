module sigmoid #(
    parameter integer DATA_W = 16,
    parameter integer FRAC = 8
) (
    input  wire signed [DATA_W-1:0] in_data,
    output reg  signed [DATA_W-1:0] out_data
);
localparam signed [DATA_W-1:0] ONE = 16'sh0100;     // 1.0
localparam signed [DATA_W-1:0] HALF = 16'sh0080;    // 0.5
localparam signed [DATA_W-1:0] POS4 = 16'sh0400;    // 4.0
localparam signed [DATA_W-1:0] NEG4 = -16'sh0400;   // -4.0

always @(*) begin
    if (in_data >= POS4)
        out_data = ONE;
    else if (in_data <= NEG4)
        out_data = 0;
    else
        out_data = HALF + (in_data >>> 3); // 0.5 + x/8
end
endmodule
