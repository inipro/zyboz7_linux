#JC1_P, ~CS
set_property PACKAGE_PIN V15 [get_ports spi_rtl_ss_io]
#JC1_N, NC
set_property PACKAGE_PIN W15 [get_ports spi_rtl_io0_io]
#JC2_P, SDO
set_property PACKAGE_PIN T11 [get_ports spi_rtl_io1_io]
#JC2_N, SCK
set_property PACKAGE_PIN T10 [get_ports spi_rtl_sck_io]

# Set the bank voltage for IO Bank 34 to 3.3V
set_property IOSTANDARD LVCMOS33 [get_ports -of_objects [get_iobanks 34]];
