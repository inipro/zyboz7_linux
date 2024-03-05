##Pcam MIPI CSI-2 Connector
## This configuration expects the sensor to use 672Mbps/lane = 336 MHz HS_Clk
#create_clock -period 2.976 -name dphy_hs_clock_clk_p -waveform {0.000 1.488} [get_ports dphy_hs_clock_clk_p]
set_property INTERNAL_VREF 0.6 [get_iobanks 35]
set_property -dict { PACKAGE_PIN J19   IOSTANDARD HSUL_12     } [get_ports { mipi_phy_if_0_clk_lp_n }]; #IO_L10N_T1_AD11N_35 Sch=lp_clk_n
set_property -dict { PACKAGE_PIN H20   IOSTANDARD HSUL_12     } [get_ports { mipi_phy_if_0_clk_lp_p }]; #IO_L17N_T2_AD5N_35 Sch=lp_clk_p
set_property -dict { PACKAGE_PIN M18   IOSTANDARD HSUL_12     } [get_ports { mipi_phy_if_0_data_lp_n[0] }]; #IO_L8N_T1_AD10N_35 Sch=lp_lane_n[0]
set_property -dict { PACKAGE_PIN L19   IOSTANDARD HSUL_12     } [get_ports { mipi_phy_if_0_data_lp_p[0] }]; #IO_L9P_T1_DQS_AD3P_35 Sch=lp_lane_p[0]
set_property -dict { PACKAGE_PIN L20   IOSTANDARD HSUL_12     } [get_ports { mipi_phy_if_0_data_lp_n[1] }]; #IO_L9N_T1_DQS_AD3N_35 Sch=lp_lane_n[1]
set_property -dict { PACKAGE_PIN J20   IOSTANDARD HSUL_12     } [get_ports { mipi_phy_if_0_data_lp_p[1] }]; #IO_L17P_T2_AD5P_35 Sch=lp_lane_p[1]
set_property -dict { PACKAGE_PIN H18   IOSTANDARD LVDS_25     } [get_ports { mipi_phy_if_0_clk_hs_n }]; #IO_L14N_T2_AD4N_SRCC_35 Sch=mipi_clk_n
set_property -dict { PACKAGE_PIN J18   IOSTANDARD LVDS_25     } [get_ports { mipi_phy_if_0_clk_hs_p }]; #IO_L14P_T2_AD4P_SRCC_35 Sch=mipi_clk_p
set_property -dict { PACKAGE_PIN M20   IOSTANDARD LVDS_25     } [get_ports { mipi_phy_if_0_data_hs_n[0] }]; #IO_L7N_T1_AD2N_35 Sch=mipi_lane_n[0]
set_property -dict { PACKAGE_PIN M19   IOSTANDARD LVDS_25     } [get_ports { mipi_phy_if_0_data_hs_p[0] }]; #IO_L7P_T1_AD2P_35 Sch=mipi_lane_p[0]
set_property -dict { PACKAGE_PIN L17   IOSTANDARD LVDS_25     } [get_ports { mipi_phy_if_0_data_hs_n[1] }]; #IO_L11N_T1_SRCC_35 Sch=mipi_lane_n[1]
set_property -dict { PACKAGE_PIN L16   IOSTANDARD LVDS_25     } [get_ports { mipi_phy_if_0_data_hs_p[1] }]; #IO_L11P_T1_SRCC_35 Sch=mipi_lane_p[1]
set_property -dict { PACKAGE_PIN G20   IOSTANDARD LVCMOS33 	PULLUP true} [get_ports { cam_gpio }]; #IO_L18N_T2_AD13N_35 Sch=cam_gpio
set_property -dict { PACKAGE_PIN F20   IOSTANDARD LVCMOS33 } [get_ports { IIC_0_scl_io }]; #IO_L15N_T2_DQS_AD12N_35 Sch=cam_scl
set_property -dict { PACKAGE_PIN F19   IOSTANDARD LVCMOS33 } [get_ports { IIC_0_sda_io }]; #IO_L15P_T2_DQS_AD12P_35 Sch=cam_sda

#HDMI TX
set_property -dict { PACKAGE_PIN G17   IOSTANDARD LVCMOS33 } [get_ports { IIC_1_scl_io }]; #IO_L16P_T2_35 Sch=hdmi_tx_scl
set_property -dict { PACKAGE_PIN G18   IOSTANDARD LVCMOS33 } [get_ports { IIC_1_sda_io }]; #IO_L16N_T2_35 Sch=hdmi_tx_sda

set_property -dict { PACKAGE_PIN H17   IOSTANDARD TMDS_33  } [get_ports { TMDS_0_clk_n }]; #IO_L13N_T2_MRCC_35 Sch=hdmi_tx_clk_n
set_property -dict { PACKAGE_PIN H16   IOSTANDARD TMDS_33  } [get_ports { TMDS_0_clk_p }]; #IO_L13P_T2_MRCC_35 Sch=hdmi_tx_clk_p
set_property -dict { PACKAGE_PIN D20   IOSTANDARD TMDS_33  } [get_ports { TMDS_0_data_n[0]  }]; #IO_L4N_T0_35 Sch=hdmi_tx_n[0]
set_property -dict { PACKAGE_PIN D19   IOSTANDARD TMDS_33  } [get_ports { TMDS_0_data_p[0]  }]; #IO_L4P_T0_35 Sch=hdmi_tx_p[0]
set_property -dict { PACKAGE_PIN B20   IOSTANDARD TMDS_33  } [get_ports { TMDS_0_data_n[1]  }]; #IO_L1N_T0_AD0N_35 Sch=hdmi_tx_n[1]
set_property -dict { PACKAGE_PIN C20   IOSTANDARD TMDS_33  } [get_ports { TMDS_0_data_p[1]  }]; #IO_L1P_T0_AD0P_35 Sch=hdmi_tx_p[1]
set_property -dict { PACKAGE_PIN A20   IOSTANDARD TMDS_33  } [get_ports { TMDS_0_data_n[2]  }]; #IO_L2N_T0_AD8N_35 Sch=hdmi_tx_n[2]
set_property -dict { PACKAGE_PIN B19   IOSTANDARD TMDS_33  } [get_ports { TMDS_0_data_p[2]  }]; #IO_L2P_T0_AD8P_35 Sch=hdmi_tx_p[2]
