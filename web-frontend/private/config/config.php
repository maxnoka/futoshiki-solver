<?php
define("DEBUG_ENABLED", False);
if (DEBUG_ENABLED) {
	ini_set('display_startup_errors', 1);
	ini_set('display_errors', 1);
	error_reporting(E_ALL);
}

// private - used only in php files
define( "TEMPLATE_PATH", __DIR__ . "/../views/templates" );
define( "CLASS_PATH", __DIR__ . "/../classes" );
define( "BIN_PATH", __DIR__ . "/../bin" );

// public - also used in templates
define( "STYLE_PATH",  "styles" );
define( "SCRIPT_PATH", "scripts" );

function handleException( $exception ) {
  echo "Sorry, a problem occurred. Please try later.";
  error_log( $exception->getMessage() );
}

set_exception_handler( 'handleException' );
?>
