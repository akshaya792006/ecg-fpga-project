module relu #(
    parameter integer N = 1,
    parameter integer DATA_W = 16
) (
    input  wire [N*DATA_W-1:0] in_data,
    output reg  [N*DATA_W-1:0] out_data
);
integer i;
always @(*) begin
    for (i = 0; i < N; i = i + 1) begin
        if (in_data[i*DATA_W + DATA_W-1])
            out_data[i*DATA_W +: DATA_W] = {DATA_W{1'b0}};
        else
            out_data[i*DATA_W +: DATA_W] = in_data[i*DATA_W +: DATA_W];
    end
end
endmodule
