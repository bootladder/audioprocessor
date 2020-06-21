#!/bin/bash
PATHTOOPENOCD=/usr/share/openocd/scripts
sudo openocd -f stm32f7discovery.cfg -s $PATHTOOPENOCD/board -s $PATHTOOPENOCD