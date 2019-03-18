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

module module_inst(
	input clk, 
	input rst, 
	input en,
	input d_1,
	input d_2, 
	output q
);

wire q_1;
wire q_2;

assign q = q_1 & q_2;

basic_ff ff_1(
	.clk(clk),
	.rst(rst),
	.en(en),
	.d(d_1),
	.q(q_1)
);

basic_ff ff_2(
	.clk(clk),
	.rst(rst),
	.en(en),
	.d(d_2),
	.q(q_2)
);

endmodule