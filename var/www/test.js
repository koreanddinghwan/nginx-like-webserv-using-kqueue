const UploadServer = "http://localhost:80/fileupload/";

const uploadBtn = document.getElementById("fileUploadBtn");
const deleteBtn = document.getElementById("fileDeleteBtn");
const checkUploadBtn = document.getElementById("checkUploadBtn");
const cgiBtn = document.getElementById("cgiBtn");
const multipleCgiBtn1 = document.getElementById("multipleCgiBtn1");
const multipleCgiBtn2 = document.getElementById("multipleCgiBtn2");

const fileUploadInput = document.getElementById("fileUploadInput");
const fileDeleteInput = document.getElementById("fileDeleteInput");
const cgiInput = document.getElementById("cgiInput");
const cgiOutput = document.getElementById("cgiOutput");
const multipleCgiOutput1 = document.getElementById("multipleCgiOutput1");
const multipleCgiOutput2 = document.getElementById("multipleCgiOutput2");

const onClickUploadBtn = async (event) => {
  const fileName = fileUploadInput.files[0].name;
  const filePath = UploadServer + fileName;

  console.log("upload");
  console.log(filePath);

  const postedResult = await fetch(filePath, {
    method: "POST",
    body: fileUploadInput.files[0],
  });
  console.log(postedResult);
  if (postedResult.ok) {
	alert("posted");
	fileDeleteInput.value = fileName;
  } else {
	alert("failed");
  }
};

const onClickDeleteBtn = async (event) => {
	const fileName = fileDeleteInput.value;
	const filePath = "http://localhost:80/delete/" + fileName;
	console.log("delete");
	console.log(filePath);
	const deletedResult = await fetch(filePath, {
		method: "DELETE",
	});
	console.log(deletedResult);
	if (deletedResult.ok) {
		alert("deleted");
	}
	else {
		alert("failed");
	}
}

const onClickCheckUploadBtn = async (event) => {
	const url = "http://localhost:80/upload/" + fileUploadInput.files[0].name;
	window.open(url, '_blank');
}

const onClickCgiBtn = async (event) => {
	const url = "http://localhost:80/cgi";
	const result = await fetch(url, 
	{
		"method": "POST",
		"body": cgiInput.value,
	});
	cgiOutput.value = await result.text();
}

const onClickMultipleCgiBtn1 = async (event) => {
	const url = "http://localhost:80/asdf.shsh";

	const result = await fetch(url,
	{
		"method": "GET"
	});
	const wrapper = document.createElement("div");
	// const doc = new DOMParser().parseFromString(await result.text(), "text/xml");
	wrapper.innerHTML = await result.text();
	multipleCgiOutput1.appendChild(wrapper);
}

const onClickMultipleCgiBtn2 = async (event) => {
	const url = "http://localhost:80/asdf.pyth";

	const result = await fetch(url,
	{
		"method": "GET"
	});
	const wrapper = document.createElement("div");
	// const doc = new DOMParser().parseFromString(await result.text(), "text/xml");
	wrapper.innerHTML = await result.text();
	multipleCgiOutput2.appendChild(wrapper);
}


uploadBtn.addEventListener("click", onClickUploadBtn);
deleteBtn.addEventListener("click", onClickDeleteBtn);
checkUploadBtn.addEventListener("click", onClickCheckUploadBtn);
cgiBtn.addEventListener("click", onClickCgiBtn);
multipleCgiBtn1.addEventListener("click", onClickMultipleCgiBtn1);
multipleCgiBtn2.addEventListener("click", onClickMultipleCgiBtn2);
