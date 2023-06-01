const UploadServer = "http://localhost:80/upload/";

const test = async (event) => {
  console.log("test");
  console.log(event);
  const fileUpload = document.getElementById("fileUploadTest");

  const fileName = fileUpload.files[0].name;
  const filePath = UploadServer + fileName;

  console.log("upload");
  console.log(filePath);

  const postedResult = await fetch(filePath, {
    method: "POST",
  });
  console.log(postedResult);
};

const uploadBtn = document.getElementById("fileUploadBtn");

uploadBtn.addEventListener("click", test);

console.log("js transfered");
