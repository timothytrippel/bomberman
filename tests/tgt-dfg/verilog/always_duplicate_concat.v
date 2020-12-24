module always_duplicate_concat(
	input clk,
	input in, 
	output reg [1:0] out
);

	always @(posedge clk) begin
		out <= {in,in};
	end

endmodule