module basic_ff(
	input clk, 
	input rst, 
	input en,
	input d, 
	output reg q
);

	wire next_q;

	assign next_q = en ? d : q;

	always @(posedge clk) begin
		if (rst)
			q <= 1'b0;
		else
			q <= next_q;
	end
endmodule
