<?php
exec('g++ main.cc -o restaurants', $output, $return);
exec('./restaurants ravintolat.csv', $output, $return);

var_dump($output);
?>
