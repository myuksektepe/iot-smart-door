$("#menu-toggle").click(function (e) {

});


$("#sidebar-wrapper .closer").on('click', function () {

    //e.preventDefault();
    $("#wrapper").toggleClass("toggled");


    if ($("#wrapper").hasClass("toggled")) {
        $("#sidebar-wrapper .closer").text("<").removeClass("c");
    } else {
        $("#sidebar-wrapper .closer").text(">").addClass("c");
    }
});