module basic_2bit_ff(input clk, input rst, input [1:0] d, input en, output reg [1:0] q);

	wire [1:0] next_q;

	assign next_q = en ? d : q;

	always @(posedge clk) begin
		if (rst)
			q <= 2'b00;
		else
			q <= next_q;
	end
endmodule
