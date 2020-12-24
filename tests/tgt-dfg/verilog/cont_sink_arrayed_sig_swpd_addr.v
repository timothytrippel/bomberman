module cont_sink_arrayed_sig(
	input  [1:0] in_1,
	input  [1:0] in_2,
	input  [1:0] in_3,
	input  [1:0] in_4,
	output [7:0] out
);	

	wire [1:0] middle [3:0];

	assign middle[0] = in_1;
	assign middle[1] = in_2;
	assign middle[2] = in_3;
	assign middle[3] = in_4;

	assign out = {middle[0], middle[1], middle[2], middle[3]};

endmodule
