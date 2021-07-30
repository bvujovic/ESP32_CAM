<?php
var_dump($_FILES);
if (isset($_FILES['image']) && $_FILES['image']['name'] != '') {
   $errors = array();
   $file_name = $_FILES['image']['name'];
   $file_size = $_FILES['image']['size'];
   $file_tmp = $_FILES['image']['tmp_name'];
   $file_type = $_FILES['image']['type'];
   $file_ext = strtolower(end(explode('.', $_FILES['image']['name'])));

   $extensions = array("jpeg", "jpg", "png");

   if (in_array($file_ext, $extensions) === false) {
      $errors[] = "extension not allowed, please choose a JPEG or PNG file.";
   }

   if ($file_size > 2097152) {
      $errors[] = 'File size must be less than 2MB';
   }

   if (empty($errors) == true) {
      move_uploaded_file($file_tmp, "images/" . $file_name);
      echo "Success";
   } else {
      print_r($errors);
   }
}
?>
<html>

<body>

   <form action="" method="POST" enctype="multipart/form-data">
      <input type="file" name="image" />
      <input type="submit" />
   </form>

</body>

</html>

array(1) { ["image"]=> array(5) { ["name"]=> string(22) "ESP32-CAM-pinouts.jpeg" ["type"]=> string(10) "image/jpeg" ["tmp_name"]=> string(14) "/tmp/phpWCJRZJ" ["error"]=> int(0) ["size"]=> int(45770) } } Success