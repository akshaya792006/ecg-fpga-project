module maxpool #(
    parameter integer IN_LEN = 185,
    parameter integer CHANNELS = 16,
    parameter integer DATA_W = 16,
    parameter integer OUT_LEN = IN_LEN/2
) (
    input  wire [IN_LEN*CHANNELS*DATA_W-1:0] in_data,
    output reg  [OUT_LEN*CHANNELS*DATA_W-1:0] out_data
);
integer c, i;
reg signed [DATA_W-1:0] a, b;
always @(*) begin
    for (c = 0; c < CHANNELS; c = c + 1) begin
        for (i = 0; i < OUT_LEN; i = i + 1) begin
            a = in_data[((2*i)*CHANNELS + c)*DATA_W +: DATA_W];
            b = in_data[((2*i+1)*CHANNELS + c)*DATA_W +: DATA_W];
            out_data[(i*CHANNELS + c)*DATA_W +: DATA_W] = (a > b) ? a : b;
        end
    end
end
endmodule
