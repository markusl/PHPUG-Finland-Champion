<?php
exec('clang++ -std=c++11 main.cc -o restaurants', $output, $return);
exec('./restaurants ravintolat.csv', $output, $return);

var_dump($output);
?>
