<!DOCTYPE html>
<html>
    <head>
        <!--Import Google Icon Font-->
        <link href="https://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">
        <!--Import materialize.css-->
        <link type="text/css" rel="stylesheet" href="css/materialize.min.css"  media="screen,projection"/>
        <!--Let browser know website is optimized for mobile-->
        <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
    </head>

    <body>
        <nav class="nav-extended" >
            <div class="nav-wrapper teal lighten-2 class" >
                <a href="#" class="brand-logo" >
                    EDJ Forked Web Server
                </a>
            </div>
        </nav>

        <?php
            $dir = (isset($_GET['dir']) ? $_GET['dir'] : "/usr/src/ws/bin/res");

            if (is_dir($dir)) {
                $files = array_diff(scandir($dir), array('..', '.'));
                usort($files, function($a, $b) {
                if(is_dir($a) == is_dir($b))
                return strnatcasecmp($a, $b);
            else
                return is_dir($a) ? -1 : 1;
            });
            echo "<div class=collection>";

            foreach($files AS $file) {
                echo "<a href='" . urlencode("{$file}") . "'class='collection-item'>{$file}</a><br>\n";               
            }

            echo "</div>";
            } else if (is_file($dir)) {
                header('Content-Description: File Transfer');
                header('Content-Type: application/octet-stream');
                header('Content-Disposition: attachment; filename="' . basename($dir) . '"');
                header('Expires: 0');
                header('Cache-Control: must-revalidate');
                header('Pragma: public');
                header('Content-Length: ' . filesize($dir));
                readfile($dir);
            }
            exit;
        ?>
        <!--JavaScript at end of body for optimized loading-->
        <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
        <script src="js/materialize.min.js"></script>
        <script src="https://www.gstatic.com/firebasejs/5.7.3/firebase.js"></script>
        
    </body>
</html>
