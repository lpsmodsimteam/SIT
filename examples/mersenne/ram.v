`timescale 1ns / 1ps

module ram #(
    parameter ADDR_WIDTH = 8,
    parameter DATA_WIDTH = 32
) (
    input wire clk,
    input wire cs,
    input wire we,
    input wire oe,
    input wire [ADDR_WIDTH - 1:0] address,
    input wire [DATA_WIDTH - 1:0] data_in,
    output reg [DATA_WIDTH - 1:0] data_out
);

  reg [DATA_WIDTH - 1:0] mem[0:(1 << ADDR_WIDTH) - 1];

  always @(posedge clk) begin

    if (cs && we) begin
      mem[address] <= data_in;
    end else if (cs && oe && !we) begin
      data_out <= mem[address];
    end

  end

endmodule
