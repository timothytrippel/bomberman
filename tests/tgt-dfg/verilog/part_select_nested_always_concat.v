module nested_always_concat(
	input clk,
	input a, 
	input b, 
	input [1:0] c,
	input [2:0] d,
	input e,
	output reg [7:0] out
);

	always @(posedge clk) begin
		out[6:1] <= {a,{e,d},b};
		out[7:7] <= c[1:1];
		out[0:0] <= c[0];
	end

endmodule