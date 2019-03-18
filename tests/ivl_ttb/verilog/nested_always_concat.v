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
		out <= {a,c,b,{e,d}};
	end

endmodule