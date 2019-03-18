module always_sink_source_part_select(
	input clk, 
	input [1:0] a, 
	output reg [1:0] out
);

	always @(posedge clk) begin
		out[0] <= a[1];
		out[1] <= a[0];
	end

endmodule