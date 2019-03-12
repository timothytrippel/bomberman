module two_bit_distr_counter(input clk, input rst, input rst2, output [1:0] count);
	reg lsb;
	reg msb;

	wire next_lsb;
	wire next_msb;

	assign next_lsb = ~lsb;
	assign next_msb = lsb ? ~msb : msb;

	assign count = {msb, lsb};

	always @(posedge clk) begin
		if (rst | rst2) begin
			lsb <= 1'b0;
			msb <= 1'b0;
		end else begin
			lsb <= next_lsb;
			msb <= next_msb;
		end
	end
endmodule
