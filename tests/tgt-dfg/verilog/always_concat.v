module always_concat(
	input clk,
	input a, 
	input b, 
	input [1:0] c,
	output reg [3:0] out
);

	always @(posedge clk) begin
		out <= {a,c,b};
	end

endmodule