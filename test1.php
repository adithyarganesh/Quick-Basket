<?php
	 		    // $output;
	 		    $result = exec("python form.py");
	 		    echo json_decode($result);
	 		    
	 		?>