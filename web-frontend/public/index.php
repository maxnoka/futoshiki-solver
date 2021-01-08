<?php
require('../private/config/config.php');
require( CLASS_PATH . "/App.php" );

$boardSize = isset($_GET['size']) ? filter_var(htmlspecialchars($_GET['size']), FILTER_VALIDATE_INT) : false;

if ($boardSize != false) {
    $app = new App(max(min($boardSize, 7), 2));   
}
else {
    $app = new App(); 
}

$app->render();
?>
