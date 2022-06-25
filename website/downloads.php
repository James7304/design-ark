<!DOCTYPE html>
<html>
<head>

<title>Design Ark</title>
<link rel="icon" href="img/logo.png">

<style>

    h2{
        font-family:'Consolas';
        font-size:30px;
    }

    div.container{

        text-align:center
    }

    div.downloadTray{

        background-color:aliceblue;
        
        border: 1px;
        border-style: solid;
        border-color: black;
        border-radius:10px;

        margin: 25px;
        padding:0px;
        padding-bottom:10px;

        display:inline-block;
    }

    div.downloadTray .title{
        margin:0px;
        font-family:'Consolas';
    }

    div.downloadTray .record{

        margin:0px;
        padding:0px;

    }

    div.downloadTray .record ul{

        list-style-type: none;
    
        margin: 0;
        padding: 3px;

    }

    div.downloadTray .record li{
        margin-left:30px;
        margin-right:30px;
        padding:0;
        display:inline-block;

        font-family:'Consolas';
    }

</style>

</head>
<body>

    <?php 
        include 'banner.html';
        include 'nav.html';
    ?>

    <h2 align="center">Downloadable Files</h2>

    <div class="container">

        <div class="downloadTray">
            <div class="title">
                <h3 align="center">Design Ark Downloads</h3>
            </div>
            <div class="record">
                <ul>
                    <li style="margin-right:5px"><img src="img/logo.png" width="25px" height="25px"></li>
                    <li style="margin-left:5px">DesignArk: Pseudo</li>
                    <li style="margin-left:5px">Windows (x64)</li>
                    <li>v1</li>
                    <li><a href="downloads/DesignArk.exe" download>Download</a></li>
                </ul>
            </div>
        </div>

        <div class="downloadTray">
            <div class="title">
                <h3 align="center">User Documentation</h3>
            </div>
            <div class="record">
                <ul>
                    <li style="margin-right:5px"><img src="img/pdfIcon.png" width="25px" height="25px"></li>
                    <li style="margin-left:5px">DesignArk: Pseudo</li>
                    <li>v1</li>
                    <li><a href="downloads/userdocs.pdf" download>Download</a></li>
                </ul>
            </div>
        </div>

    </div>
    
    <?php
        include 'footer.html';
    ?>

    
    <script>
        // Page dependant script
        document.getElementById('downloads').style="background-color: rgb(175, 73, 73);";

        document.getElementById('footer-link-first').innerHTML='Home';
        document.getElementById('footer-link-second').innerHTML='User-Docs';
        document.getElementById('footer-link-first').href=".";
        document.getElementById('footer-link-second').href="userdocs.php";

        document.getElementById('footer').style="position:fixed; bottom:0; width:100%;"
        document.getElementById('topButton').remove();
    </script>

</body>
</html>
