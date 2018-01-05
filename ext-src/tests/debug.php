<?php

require_once './libs.php';

class Ext{
    public function next($a){
	    sleep(1);
	    return $a;
	}
}
function test($a){
    $foo = new Ext;
    sleep(1);
    return $foo->next($a);
}
test("123");
sleep(1);
libs();
