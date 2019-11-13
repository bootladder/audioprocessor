#!/bin/bash
PATHTOOPENOCD=/home/steve/Ac6/SystemWorkbench/plugins/fr.ac6.mcu.debug_2.5.0.201904120827/resources/openocd/scripts
PATHTOBIN=/home/steve/Ac6/SystemWorkbench/plugins/fr.ac6.mcu.externaltools.openocd.linux64_1.23.0.201904120827/tools/openocd/bin/openocd
sudo $PATHTOBIN -f stm32f769i_disco.cfg -s $PATHTOOPENOCD/st_board -s $PATHTOOPENOCD
