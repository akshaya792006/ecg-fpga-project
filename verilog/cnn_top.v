module cnn_top #(
    parameter integer DATA_W = 16,
    parameter integer FRAC = 8
) (
    input  wire clk,
    input  wire rst,
    input  wire start,
    input  wire [187*DATA_W-1:0] ecg_in,
    output wire done,
    output wire [DATA_W-1:0] prob_out
);
wire conv1_done, conv2_done, dense1_done, dense2_done;
wire [185*16*DATA_W-1:0] conv1_out, conv1_relu;
wire [92*16*DATA_W-1:0] pool1_out;
wire [90*32*DATA_W-1:0] conv2_out, conv2_relu;
wire [45*32*DATA_W-1:0] pool2_out;
wire [1440*DATA_W-1:0] flat_out;
wire [64*DATA_W-1:0] dense1_out, dense1_relu;
wire [DATA_W-1:0] dense2_out;

conv1d_layer1 u_conv1(.clk(clk), .rst(rst), .start(start), .in_data(ecg_in), .done(conv1_done), .out_data(conv1_out));
relu #(.N(185*16), .DATA_W(DATA_W)) u_relu1(.in_data(conv1_out), .out_data(conv1_relu));
maxpool #(.IN_LEN(185), .CHANNELS(16), .DATA_W(DATA_W), .OUT_LEN(92)) u_pool1(.in_data(conv1_relu), .out_data(pool1_out));

conv1d_layer2 u_conv2(.clk(clk), .rst(rst), .start(conv1_done), .in_data(pool1_out), .done(conv2_done), .out_data(conv2_out));
relu #(.N(90*32), .DATA_W(DATA_W)) u_relu2(.in_data(conv2_out), .out_data(conv2_relu));
maxpool #(.IN_LEN(90), .CHANNELS(32), .DATA_W(DATA_W), .OUT_LEN(45)) u_pool2(.in_data(conv2_relu), .out_data(pool2_out));

flatten #(.IN_LEN(45), .IN_CH(32), .DATA_W(DATA_W), .OUT_LEN(1440)) u_flat(.in_data(pool2_out), .out_data(flat_out));

dense_layer #(.DIN(1440), .DOUT(64), .DATA_W(DATA_W), .FRAC(FRAC), .LAYER_ID(1))
u_dense1(.clk(clk), .rst(rst), .start(conv2_done), .in_data(flat_out), .done(dense1_done), .out_data(dense1_out));

relu #(.N(64), .DATA_W(DATA_W)) u_relu3(.in_data(dense1_out), .out_data(dense1_relu));

dense_layer #(.DIN(64), .DOUT(1), .DATA_W(DATA_W), .FRAC(FRAC), .LAYER_ID(2))
u_dense2(.clk(clk), .rst(rst), .start(dense1_done), .in_data(dense1_relu), .done(dense2_done), .out_data(dense2_out));

sigmoid u_sigmoid(.in_data(dense2_out), .out_data(prob_out));
assign done = dense2_done;

endmodule
