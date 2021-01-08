<?php
require('../private/config/config.php');
require( CLASS_PATH . "/Game.php" );

// required headers
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json; charset=UTF-8");
header("Access-Control-Allow-Methods: POST");
header("Access-Control-Max-Age: 3600");
header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");

$game = Game::createFromJson(file_get_contents("php://input"));

$out = $game->solve();
if ($out == "not valid") {
	http_response_code(400);
    echo json_encode(array("message" => "Unable to solve. Board not solveable."));
}
else {
	http_response_code(200);
	echo json_encode($out);
}

?>