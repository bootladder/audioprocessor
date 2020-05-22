MEMORY
{
MY_APP (rx)      : ORIGIN = 0x08100000, LENGTH = 1024K
}

SECTIONS
{
  /* ONLY LOAD MY APP */
  .my_app :
  {

  } >MY_APP

}


