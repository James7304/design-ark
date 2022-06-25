<!DOCTYPE html>
<html>
<head>

<title>Design Ark</title>
<link rel="icon" href="img/logo.png">

<style>

.viewer{
    width:75%;
    height: 900px;
}
div .title{
    font-family:'Consolas';
    font-size:30px;
}
div .link{
    font-family:'Consolas';
}

</style>

</head>
<body>

    <?php 
        include 'banner.html';
        include 'nav.html';
    ?>

    <div>
        <h2 align="center" class="title">User Documentation for Design Ark: Pseudo</h2>
        <p align="center" class="link"><a href="downloads/userdocs.pdf" target="_blank">View in Fullscreen</a></p>
    </div>

    <p align="center">
        <iframe class="viewer" src="downloads/userdocs.pdf"></iframe>
    </p>

    <?php
        include 'footer.html';
    ?>

    
    <script>
        // Page dependant script
        document.getElementById('userdocs').style="background-color: rgb(175, 73, 73);";

        document.getElementById('footer-link-first').innerHTML='Home';
        document.getElementById('footer-link-second').innerHTML='Downloads';
        document.getElementById('footer-link-first').href=".";
        document.getElementById('footer-link-second').href="downloads.php";
    </script>

</body>
</html>
