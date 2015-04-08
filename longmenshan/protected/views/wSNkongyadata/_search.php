<?php
/* @var $this WSNkongyadataController */
/* @var $model WSNkongyadata */
/* @var $form CActiveForm */
?>

<div class="wide form">

<?php $form=$this->beginWidget('CActiveForm', array(
	'action'=>Yii::app()->createUrl($this->route),
	'method'=>'get',
)); ?>

	<div class="row">
		<?php echo $form->label($model,'DID'); ?>
		<?php echo $form->textField($model,'DID'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'SID'); ?>
		<?php echo $form->textField($model,'SID'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'DataTime'); ?>
		<?php echo $form->textField($model,'DataTime',array('size'=>50,'maxlength'=>50)); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'S1'); ?>
		<?php echo $form->textField($model,'S1',array('size'=>50,'maxlength'=>50)); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'S2'); ?>
		<?php echo $form->textField($model,'S2',array('size'=>50,'maxlength'=>50)); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'R1'); ?>
		<?php echo $form->textField($model,'R1',array('size'=>50,'maxlength'=>50)); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'R2'); ?>
		<?php echo $form->textField($model,'R2',array('size'=>50,'maxlength'=>50)); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'IsWarning'); ?>
		<?php echo $form->textField($model,'IsWarning',array('size'=>50,'maxlength'=>50)); ?>
	</div>

	<div class="row buttons">
		<?php echo CHtml::submitButton('Search'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- search-form -->