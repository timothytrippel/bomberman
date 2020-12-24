module signed_ext_concat(
	input clk, 
	input [1:0] a, 
	input [1:0] b, 
	output reg [7:0] c);

	always @(posedge clk) begin
		c[5:0] <= $signed({a, b});
	end
endmodule