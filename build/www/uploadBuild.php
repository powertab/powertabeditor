<?php

$username = 'username';
$password = 'password';

$filesToKeep = 30;
$uploadDirectory =dirname(__FILE__) . '/builds';
$uploadedFilename = $uploadDirectory . 
                    '/' . 
                    basename(@$_FILES['build']['name']);

// basic sanity checks of content
if (@$_POST['username'] !== $username)
{
    echo "Invalid username.\n";
}
elseif (@$_POST['password'] !== $password)
{
    echo "Invalid password.\n";
}
elseif (!@$_FILES['build']['name'])
{
    echo "No file supplied.\n";
}
elseif (@$_FILES['build']['error'] > 0)
{
    echo 'Error: ' . $_FILES['build']['error'] . ".\n";
}
elseif (!move_uploaded_file($_FILES['build']['tmp_name'], $uploadedFilename))
{
    echo "Unable to move uploaded file\n";
}
else
{
    // keep the oldest <n> files
    if ($dirHandle = opendir($uploadDirectory))
    {
	echo "File uploaded successfully.\n";
        $filesByTime = array();

	echo "\nChecking for files to remove.\n";
        while (false !== ($filename = readdir($dirHandle)))
        {
            $filePath = "$uploadDirectory/$filename";
            if (is_file($filePath))
            {
                $fileCTime = filectime($filePath);
                echo "Found file $filePath ($fileCTime).\n";

                // fudge the keys in case of duplicates
                while(isset($filesByTime[$fileCTime]))
                {
                    ++$fileCTime;
                }
                $filesByTime[$fileCTime] = $filePath;
            }
        }

        // sort by timestamp - old first
        ksort($filesByTime, SORT_NUMERIC);
        $filesToRemove = count($filesByTime) - $filesToKeep;
        echo "\nRemoving $filesToRemove files.\n";
        while ($filesToRemove > 0 &&
               list($fileCTime, $filePath) = each($filesByTime))
        {
            echo "Removing $filePath\n";
            unlink($filePath);
            --$filesToRemove;
        }

        echo "\nFinished.\n";
        closedir($dirHandle);
    }
    else
    {
        echo "Failed to open dir.\n";
    }
}

?>
