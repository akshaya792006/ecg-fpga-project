module flatten #(
    parameter integer IN_LEN = 45,
    parameter integer IN_CH = 32,
    parameter integer DATA_W = 16,
    parameter integer OUT_LEN = IN_LEN*IN_CH
) (
    input  wire [IN_LEN*IN_CH*DATA_W-1:0] in_data,
    output wire [OUT_LEN*DATA_W-1:0] out_data
);
assign out_data = in_data;
endmodule
