module const_rst_ff(input clk, input rst, input en, input d, output reg q);

	wire next_q;

	assign next_q = en ? d : q;

	always @(posedge clk) begin
		if (rst & 1'b1)
			q <= 1'b1;
		else
			q <= next_q;
	end
endmodule
