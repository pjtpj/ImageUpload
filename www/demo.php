<?php

	$HTTPHost     = $_SERVER['HTTP_HOST'];
	$ErrorMessage = "";		
	$Action       = isset($_REQUEST['Action']) ? $_REQUEST['Action'] : "";
	
	// Process Action field or submit button
	
	if($Action == "DeleteImage" || isset($_REQUEST['DeleteImage']))
	{
		// Delete the selected image - this code is not specific to dcupload

		if(!isset($_REQUEST['ImageFile']))
			exit("Action DeleteImage requires argument ImageFile");
			
		// Delete image, then drop through to refresh. 
		$imageFile = sprintf("./uploaded-images/%s", $_REQUEST['ImageFile']);
		if(!file_exists($imageFile))
		{
			$ErrorMessage = sprintf("ERROR: Image file '%s' does not exist", $imageFile);
		}
		else
		{
			unlink($imageFile);
			$ErrorMessage = sprintf("Image file '%s' was deleted", $_REQUEST['ImageFile']);
		}
	}
	
	if($Action == "ProcessImage")
	{
		// Handle the .jpg file uploaded by dcupload

		// See http://www.php.net/features.file-upload for an explanation of PHP's file
		// upload support. There is nothing PHP specific about dcupload. No special
		// server module is required - the file is sent via a standard MIME encoded
		// HTTP POST. Any server lanaguage than can decode MIME encoded POST data
		// can use dccontrol.
		
		// The "browser" that submits this form to the server is the dcupload control. 
		// The control doesn't understand HTML - the only response it can handle
		// is a simple text only response - see below.
		
		// The name of the POST form variable used by dcupload to upload the 
		// image file's contents is always "userfile":
		if(!isset($_FILES['userfile']))
		{
			// Anything other than 200 is an error
			print("201 No file was submitted");
			exit();
		}
		
		// dccontrol uses a pretty good image processing library and could accept 
		// other file formats...
		$fileParts = pathinfo($_FILES['userfile']['name']);
		$fileExtension = strtolower($fileParts['extension']);
		if($fileExtension != "jpg")
		{
			print("202 Only .jpg files are allowed");
			exit();
		}
		
		// The file names created by most digital camera files are not all that handy or 
		// unique. Here we generate random unique file names. In a typical application,
		// images more often have IDs associated with database records. 
		$uniqueName = md5(uniqid(time())) . ".jpg";
		$srcFile    = $_FILES['userfile']['tmp_name'];
		// The ./ in the path is required by move_uploaded_file
		$destFile   = sprintf("./uploaded-images/%s", $uniqueName);
		// If you don't use @, errors and warnings cause HTML output
		if(! @move_uploaded_file($srcFile, $destFile))
		{
			$errMsg = $php_errormsg;
			print("203 Cannot move file from '$srcFile' to '$destFile'. The system error is: $errMsg");
			exit();
		}
	
		// 200 Response is required by dcupload control to indicate success
		printf("200 The file was successfully uploaded.");
		exit();
	}
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>
dcupload Demo
</title>
</head>
<body>
<form name="Demo" method="POST" action="demo.php">

	<?php if($ErrorMessage): ?>
	<p><font size="5" color="#008000"><?php echo $ErrorMessage; ?></font></p>
	<?php endif; ?>
	
	<table border="1" cellpadding="0" cellspacing="0">
	<?php 
		$imageDir = dir("uploaded-images");
		while(false !== ($imageEntry = $imageDir->read()))
		{
			if($imageEntry == "." || $imageEntry == "..")
				continue;
	?>
	<tr>
		<td><img src='<?php printf("uploaded-images/%s", $imageEntry); ?>'></img></td>
		<td><?php echo $imageEntry; ?></td>
		<td><a href='<?php echo "http://$HTTPHost/demo.php?Action=DeleteImage&ImageFile=$imageEntry"; ?>'>Delete</a></td>
	</tr>
	<?php
		}
		$imageDir->close();
	?>
	</table>
	
	<INPUT TYPE=BUTTON VALUE="Add Image" NAME="AddImage" OnClick="AddImage_OnClick()">
	
    <div style="display: none">
        <object id="dcuctl" classid="clsid:652FE09F-3289-414A-B1D7-2E661FA6119B" 
        	codebase='<?php echo "http://$HTTPHost/controls/ImageUploadCtrl.cab#version=1,0,0,1"; ?>'
            width="0" height="0" visible="false">
        </object>
    </div>
    
</body>

<SCRIPT LANGUAGE="JavaScript">
<!--
function AddImage_OnClick()
{
	Demo.dcuctl.DesignWidth = 255;
	Demo.dcuctl.DesignHeight = 160;
	Demo.dcuctl.CameraType = 4; // 4 = Computer Folder
	Demo.dcuctl.UploadImageURL = '<?php echo "http://$HTTPHost/demo.php?Action=ProcessImage"; ?>';
	Demo.dcuctl.GetImage(); // This call blocks until the Get Image dialog is closed
	Demo.submit();          // The call to submit causes the page to be refreshed
}
-->
</SCRIPT>

</html>
