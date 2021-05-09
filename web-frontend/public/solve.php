<?php
require('../private/config/config.php');
require( CLASS_PATH . "/Game.php" );

// required headers
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json; charset=UTF-8");
header("Access-Control-Allow-Methods: POST");
header("Access-Control-Max-Age: 3600");
header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");

$url = 'http://localhost:18080/solve';
$options = array(
    'http' => array(
        'header'  => "Content-type: application/x-www-form-urlencoded\r\n",
        'method'  => 'POST',
        'content' => http_build_query(file_get_contents("php://input"))
    )
);
$context  = stream_context_create($options);
$result = file_get_contents($url, false, $context);

var_dump($result);

if ($result == FALSE) {
	http_response_code(400);
    echo json_encode(array("message" => "Unable to solve. Board not solveable."));
}
else {
    echo json_encode($result);
}


?>
