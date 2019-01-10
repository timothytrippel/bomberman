`include "uart_defines.v"

module counter(input wire clk, output wire[`CNT_WIDTH - 1:0] count);
  reg [`CNT_WIDTH - 1:0] counter = `CNT_WIDTH'd0;
  assign count = counter;

  always @(posedge clk) begin
    if (counter == `CNT_WIDTH'h`CNT_MAX) begin
      counter <= #1 `CNT_WIDTH'h0;
    end else begin
      counter <= #1 counter + `CNT_WIDTH'h1;
    end
  end
endmodule
