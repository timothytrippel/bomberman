module ff_wternary_expr(
	input a, 
	input b, 
	input c, 
	input data, 
	input en, 
	input clk, 
	output reg out
);

	wire next_out;

	assign next_out = en ? data : out;

	always @(posedge clk) begin
		if (a | b & c)
			out <= 1'b0;
		else
			out <= next_out;
	end
endmodule
