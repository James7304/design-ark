<!DOCTYPE html>
<html>
<head>

<title>Design Ark</title>
<link rel="icon" href="img/logo.png">

<style>
    div.feature-block{

        padding:5px;
        margin:10px;

        text-align:center;

    }
    .feature{
        text-align:center; 
        display:inline-block;

        padding:10px;
        margin:10px;
    }
    .feature p{
        font-size:15px;
        font-family:'Consolas';
    }
    .feature h3{
        font-size:20px;
        font-family:'Consolas';
    }
    .intro{
        font-size:20px;
        font-family:'Consolas';
    }
</style>

</head>
<body>

    <?php 
        include 'banner.html';
        include 'nav.html';
    ?>

    <p align="center" class='intro'>
        DesignArk: Pseudo is an app which allows <br>
        the user to write pseudocode quickly and effectivley. <br>
    </p>

    <div class="feature-block">

        <div class="feature">
            <img src="img/SmartColour.png" alt="SmartColour" width="390" height="360">
            <h3>SmartColour</h3>
            <p><i>SmartColour allows you to add and change keywords, <br>
                and their colours,  in your pseudo code depending on your preferred writing style</i></p>
        </div>

        <div class="feature">
            <img src="img/Sublines.png" alt="Sublines" width="390" height="360">
            <h3>Subline Implementation</h3>
            <p><i>Subline implementation allows you to add and edit sublines seamlessly <br>
            in order to write your pseudo code effectivley</i></p>
        </div>

        <div class="feature">
            <img src="img/Printable.png" alt="Printing" width="390" height="360">
            <h3>Easy Printing and Exporting</h3>
            <p><i>DesignArk: Pseudo can effectivley print your pseudo code files <br>
            and export them to PDF format</i></p>
        </div>

    </div>
    
    <?php
        include 'footer.html';
    ?>

    <script>
        // Page dependant script
        document.getElementById('home').style="background-color: rgb(175, 73, 73);";

        document.getElementById('footer-link-first').innerHTML='Downloads';
        document.getElementById('footer-link-second').innerHTML='User-Docs';
        document.getElementById('footer-link-first').href="downloads.php";
        document.getElementById('footer-link-second').href="userdocs.php";
    </script>

</body>
</html>
