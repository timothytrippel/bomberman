module submod(
	input in,
	output reg [7:0] out
);

	always @(posedge in) begin
		out <= 8'hAD;
	end

endmodule

module looped_back_outputs(
	input in,
	output [31:0] out
);

	submod X (
		.in(in),
		.out(out[31:24])
	);

	assign out[23:16] = out[31:24];

	submod Y (
		.in(in),
		.out(out[7:0])
	);

endmodule