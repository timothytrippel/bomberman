module always_multi_event(
	input enable_1,
	input enable_2,
	input a, 
	input b,
	input c,
	input d,
	output reg [1:0] out
);
	
	wire enable_local;

	assign enable_local = a & b;

	always @(enable_1 or enable_2 or enable_local) begin
		out <= {c,d};
	end

endmodule