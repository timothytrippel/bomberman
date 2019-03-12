module ff_wbinary_expr(
	input a, 
	input b, 
	input data,
	input en, 
	input clk, 
	output reg out
);

	wire next_out;

	assign next_out = en ? data : out;

	always @(posedge clk) begin
		if (a | b)
			out <= 1'b0;
		else
			out <= next_out;
	end
endmodule
