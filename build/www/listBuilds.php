<!DOCTYPE html>
<html>
<header><title>Powertab Editor Dev Builds</title></header>
<body>
<h1>Powertab Editor Dev Builds</h1>
<?php
$buildsDirectory = dirname(__FILE__) . '/builds';

if ($dirHandle = @openDir($buildsDirectory))
{
    // get all files and time
    $filesByTime = array();
    while (false !== ($filename = readdir($dirHandle)))
    {
        $filePath = "$buildsDirectory/$filename";
        if (is_file($filePath))
        {
            $fileCTime = filectime($filePath);

            // fudge the keys in case of duplicates
            while(isset($filesByTime[$fileCTime]))
            {
                ++$fileCTime;
            }
            $filesByTime[$fileCTime] = $filename;
        }
    }
    closedir($dirHandle); 

    // display recent files first
    if (count($filesByTime))
    {
        ksort($filesByTime, SORT_NUMERIC);
        $filesByTime = array_reverse($filesByTime);
        echo "<ol>\n";
        while (list(, $filename) = each($filesByTime))
        {
            $filePath = "$buildsDirectory/$filename";
            $fileCTime = gmdate("d-M-Y H:i:s e", filectime($filePath));
            $fileUrl = "builds/$filename";
            echo "<li>\n";
            echo "<a href=\"$fileUrl\">$filename</a> ($fileCTime)\n";
            echo "</li>\n";
        }
        echo "</ol>\n";
    }
    else
    {
        echo "No build files.\n";
    }
}
else
{
    echo "No build directory.\n";
}

?>
</body>
</html>
