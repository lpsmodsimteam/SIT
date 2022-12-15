`timescale 1ns / 1ps

module ram #(
    parameter ADDR_WIDTH = 8,
    parameter DATA_WIDTH = 8
) (
    input wire [ADDR_WIDTH - 1:0] address,
    input wire [DATA_WIDTH - 1:0] data_in,
    output wire [DATA_WIDTH - 1:0] data_out,
    input wire cs,
    input wire we,
    input wire oe
);

  reg [DATA_WIDTH - 1:0] mem[0:(1 << ADDR_WIDTH) - 1];
  assign data_out = (cs && oe && !we) ? mem[address] : {DATA_WIDTH{1'bz}};

  always @(cs or we or oe or address) begin

    if (cs && we) begin
      mem[address] <= data_in;
    end

  end

endmodule
