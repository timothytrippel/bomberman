module always_source_part_select(
	input clk, 
	input [1:0] a, 
	output reg out
);

	always @(posedge clk) begin
		out <= a[1];
	end

endmodule