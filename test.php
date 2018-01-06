<?php
   session_start();
?>	
<html>
<head>Test</head>
<body>

	 <form>
	 	<div id='searchbox' name='searchbox' style="width: 200; border:solid; border-color: black">
	 		<?php
	 		    $output;
	 		    $result exec("python form.py");
	 		    echo $result;
	 		    
	 		?>
	 	</div>
	 	<!-- <button type="submit"> -->
	</form>
</body>
</html>