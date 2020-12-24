module always_rval_signal_select(
	input            clk,
	input      [1:0] select, 
	output reg [7:0] out
);

wire [7:0] data [3:0];

always @(posedge clk) begin
	out <= data[select];
end

endmodule
